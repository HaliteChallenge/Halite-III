"""Make usernames unique

Revision ID: 7eda33b8c7d1
Revises: 34a1aa1eea9f
Create Date: 2018-09-24 19:36:08.913437+00:00

"""
from alembic import op
import sqlalchemy as sa
from sqlalchemy.dialects import postgresql


# revision identifiers, used by Alembic.
revision = '7eda33b8c7d1'
down_revision = '34a1aa1eea9f'
branch_labels = None
depends_on = None


def upgrade():
    op.create_unique_constraint("uq_user_username", "user", ["username"])


def downgrade():
    op.drop_constraint("uq_user_username", "user")
