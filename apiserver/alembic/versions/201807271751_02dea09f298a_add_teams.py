"""Add teams

Revision ID: 02dea09f298a
Revises: b298f5f566fa
Create Date: 2018-07-27 17:51:21.259443+00:00

"""
from alembic import op
import sqlalchemy as sa
from sqlalchemy.dialects import mysql


# revision identifiers, used by Alembic.
revision = '02dea09f298a'
down_revision = 'b298f5f566fa'
branch_labels = None
depends_on = None


def upgrade():
    op.create_table('team',
                    sa.Column('id', sa.Integer(), nullable=False),
                    sa.Column('created', sa.TIMESTAMP(timezone=True), server_default=sa.text('CURRENT_TIMESTAMP'), nullable=False),
                    sa.Column('name', sa.String(length=32), server_default=sa.text('CURRENT_TIMESTAMP'), nullable=False),
                    sa.Column('verification_code', sa.String(length=32), nullable=False),
                    sa.Column('leader_id', sa.Integer(), autoincrement=False, nullable=False),
                    sa.ForeignKeyConstraint(['leader_id'], ['user.id'], name='team_leader_id_user_ibfk', ondelete='CASCADE'),
                    sa.PrimaryKeyConstraint('id'),
    )
    op.create_table('team_member',
                    sa.Column('team_id', sa.Integer(), autoincrement=False, nullable=False),
                    sa.Column('user_id', sa.Integer(), autoincrement=False, nullable=False),
                    sa.ForeignKeyConstraint(['team_id'], ['team.id'], name='team_member_team_fk', ondelete='CASCADE'),
                    sa.ForeignKeyConstraint(['user_id'], ['user.id'], name='team_member_user_ibfk', ondelete='CASCADE'),
                    sa.PrimaryKeyConstraint('team_id', 'user_id'),
    )


def downgrade():
    op.drop_table('team_member')
    op.drop_table('team')
