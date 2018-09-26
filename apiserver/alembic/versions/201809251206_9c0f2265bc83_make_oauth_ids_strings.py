"""Make OAuth IDs strings

Revision ID: 9c0f2265bc83
Revises: 7eda33b8c7d1
Create Date: 2018-09-25 12:06:48.419018+00:00

"""
from alembic import op
import sqlalchemy as sa
from sqlalchemy.dialects import postgresql


# revision identifiers, used by Alembic.
revision = '9c0f2265bc83'
down_revision = '7eda33b8c7d1'
branch_labels = None
depends_on = None


def upgrade():
    op.alter_column(
        'user', 'oauth_id', 
        type_=sa.String(length=50), 
        postgresql_using='CAST(oauth_id AS VARCHAR)')


def downgrade():
    op.alter_column(
        'user', 'oauth_id', 
        type_=sa.Integer(), 
        postgresql_using='CAST(oauth_id AS INTEGER)')
